<?php

namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\Filters;
use Plenty\Modules\DataExchange\Models\FormatSetting;

/**
 * Class IdealoDE
 * @package ElasticExport\Filters
 */
class IdealoDE extends Filters
{

    public function generateFilters(array $formatSettings = []):array
    {
		$searchFilter = [
			'variationBase.isActive?' => [],
		];

		return $searchFilter;
    }
}
