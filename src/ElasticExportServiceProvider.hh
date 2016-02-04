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
					'itemBase'=>['id'],
					'itemDescription'=>['name1'],
					'variationRetailPrice' => [
							'price'
					]
			],
			BilligerGenerator::class,
			[]
		);
	}
}