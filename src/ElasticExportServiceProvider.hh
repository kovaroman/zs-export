<?hh // strict
namespace ElasticExport;

use ElasticExport\Generators\BilligerGenerator;
use Plenty\Modules\DataExchange\Services\ExportPresetContainer;
use Plenty\Plugin\DataExchangeServiceProvider;

class ElasticExportServiceProvider extends DataExchangeServiceProvider
{
	public function register():void
	{

	}

	public function exports(ExportPresetContainer $container):void
	{

		$container->add(
            'BilligerFormat',
            [
                'itemBase'=> [
                    'id',
                    'producer',
                ],

				'itemDescription' => [
					'params' => [
						'language' => 'de', // TODO change
					],
					'fields' => [
						'name1',
						'description',
						'shortDescription',
                    	'technicalData',
					],
				],

				'variationImageList' => [
	                'params' => [
	                        'type' => 'all', // Imageurl1  to Imageurl5
							// 'referenceMarketplace' => get reference, // TODO change with format setting option
	                ],
	                'fields' => [
	                    'imageId',
	                    'type',
	                    'fileType',
	                    'path',
	                    'position',
	                    'lastUpdateTimestamp',
	                    'createTimestamp',
	                    'cleanImageName',
	                ]

				],

				'variationBase' => [
					'availability'
				],

				'variationRetailPrice' => [
					'price'
				],

                'variationStandardCategory' => [
            		'params' => [
            			'plentyId' => 1000 // TODO change with format setting option
            		],
            		'fields' => [
            			'categoryId'
            		],
            	],

				'variationBarcode' => [
					'params' => [
						'EAN' => [
							'barcodeType' => 'EANGeneral',
						],
					],
					'fields' => [
						'code',
						'createdTimestamp',
						'barcodeId',
					]
				],
            ],
            BilligerGenerator::class,
            []
        );
	}
}
