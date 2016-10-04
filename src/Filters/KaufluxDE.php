<?php

namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\Filters;
use Plenty\Modules\DataExchange\Models\FormatSetting;

/**
 * Class KaufluxDE
 * @package ElasticExport\Filters
 */
class KaufluxDE extends Filters
{
    public function generateFilters(array $formatSettings = []):array
    {
		$searchFilter = [
			'variationBase.isActive?' => [],
		];

		return $searchFilter;
    }
}
